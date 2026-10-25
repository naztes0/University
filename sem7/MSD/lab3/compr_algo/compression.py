from dataclasses import dataclass

import numpy as np

import debug_log as debug


@dataclass
class Result:
    image: np.ndarray
    psnr: float
    kept: int
    total: int


def _keep_largest(values: np.ndarray, percent: float) -> tuple[np.ndarray, int]:
    flat = values.ravel()
    count = max(1, round(flat.size * percent / 100))
    if count >= flat.size:
        return values.copy(), flat.size

    indices = np.argpartition(np.abs(flat), -count)[-count:]
    compressed = np.zeros_like(flat)
    compressed[indices] = flat[indices]
    return compressed.reshape(values.shape), count


def _psnr(original: np.ndarray, restored: np.ndarray) -> float:
    mse = np.mean((original.astype(float) - restored.astype(float)) ** 2)
    return float("inf") if mse == 0 else 10 * np.log10(255**2 / mse)


def _pad(image: np.ndarray, multiple: int = 8) -> np.ndarray:
    height, width = image.shape
    pad_h = (-height) % multiple
    pad_w = (-width) % multiple
    return np.pad(image, ((0, pad_h), (0, pad_w)), mode="edge")


def _dct_matrix(size: int = 8) -> np.ndarray:
    x = np.arange(size)
    u = x[:, None]
    scale = np.full((size, 1), np.sqrt(2 / size))
    scale[0] = 1 / np.sqrt(size)
    return scale * np.cos((2 * x + 1) * u * np.pi / (2 * size))


def compress_dct(image: np.ndarray, percent: float) -> Result:
    height, width = image.shape
    source = _pad(image).astype(float) - 128
    coefficients = np.empty_like(source)
    transform = _dct_matrix()

    for y in range(0, source.shape[0], 8):
        for x in range(0, source.shape[1], 8):
            block = source[y : y + 8, x : x + 8]
            dct_block = transform @ block @ transform.T
            coefficients[y : y + 8, x : x + 8] = dct_block
            if y == 0 and x == 0:
                debug.dct_forward(block + 128, block, dct_block)

    debug.coefficients_before("DCT", coefficients, percent)
    coefficients, kept = _keep_largest(coefficients, percent)
    debug.coefficients_after(coefficients, kept, coefficients.size)
    restored = np.empty_like(source)

    for y in range(0, source.shape[0], 8):
        for x in range(0, source.shape[1], 8):
            block = coefficients[y : y + 8, x : x + 8]
            restored_block = transform.T @ block @ transform
            restored[y : y + 8, x : x + 8] = restored_block
            if y == 0 and x == 0:
                debug.dct_inverse(restored_block + 128)

    restored = np.clip(np.rint(restored[:height, :width] + 128), 0, 255).astype(np.uint8)
    result = Result(restored, _psnr(image, restored), kept, coefficients.size)
    debug.summary("DCT", result.psnr, result.kept, result.total)
    return result


def _haar_forward(values: np.ndarray, levels: int) -> np.ndarray:
    result = values.astype(float).copy()
    height, width = result.shape
    root_two = np.sqrt(2)

    for _ in range(levels):
        area = result[:height, :width]
        rows = np.empty_like(area)
        rows[:, : width // 2] = (area[:, 0::2] + area[:, 1::2]) / root_two
        rows[:, width // 2 :] = (area[:, 0::2] - area[:, 1::2]) / root_two
        if height == result.shape[0]:
            debug.haar_forward(area[0, :8], rows[0, :4], rows[0, width // 2 : width // 2 + 4])

        result[: height // 2, :width] = (rows[0::2] + rows[1::2]) / root_two
        result[height // 2 : height, :width] = (rows[0::2] - rows[1::2]) / root_two
        height //= 2
        width //= 2

    return result


def _haar_inverse(values: np.ndarray, levels: int) -> np.ndarray:
    result = values.astype(float).copy()
    full_height, full_width = result.shape
    root_two = np.sqrt(2)

    for level in range(levels - 1, -1, -1):
        height = full_height // (2**level)
        width = full_width // (2**level)
        area = result[:height, :width]

        columns = np.empty_like(area)
        top = area[: height // 2]
        bottom = area[height // 2 :]
        columns[0::2] = (top + bottom) / root_two
        columns[1::2] = (top - bottom) / root_two
        if level == 0:
            debug.haar_inverse(top[0, :4], bottom[0, :4], columns[0, :4], columns[1, :4])

        left = columns[:, : width // 2]
        right = columns[:, width // 2 :]
        result[:height, :width:2] = (left + right) / root_two
        result[:height, 1:width:2] = (left - right) / root_two

    return result


def compress_dwt(image: np.ndarray, percent: float, levels: int = 3) -> Result:
    height, width = image.shape
    source = _pad(image, 2**levels).astype(float)
    coefficients = _haar_forward(source, levels)
    debug.coefficients_before("DWT", coefficients, percent)
    coefficients, kept = _keep_largest(coefficients, percent)
    debug.coefficients_after(coefficients, kept, coefficients.size)
    restored = _haar_inverse(coefficients, levels)
    restored = np.clip(np.rint(restored[:height, :width]), 0, 255).astype(np.uint8)
    result = Result(restored, _psnr(image, restored), kept, coefficients.size)
    debug.summary("DWT", result.psnr, result.kept, result.total)
    return result
