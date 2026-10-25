import numpy as np


def _show(title, values):
    print(f"{title}:\n{np.array2string(np.asarray(values), precision=2, suppress_small=True)}")


def dct_forward(original, centered, coefficients):
    print("\n=== DCT: перший блок 8x8 ===")
    _show("Пікселі", original)
    _show("Після зсуву на 128", centered)
    _show("DCT-коефіцієнти", coefficients)


def coefficients_before(method, coefficients, percent):
    print(f"\n=== {method}: відбір найбільших {percent}% ===")
    _show("Перші коефіцієнти до відбору", coefficients[:8, :8])


def coefficients_after(coefficients, kept, total):
    _show("Перші коефіцієнти після відбору", coefficients[:8, :8])
    print(f"Збережено: {kept} / {total} ({kept / total * 100:.1f}%)")


def dct_inverse(restored):
    _show("Перший блок після оберненого DCT", restored)


def haar_forward(source, averages, details):
    print("\n=== Haar: перший рядок, перший рівень ===")
    _show("Початкові 8 пікселів", source)
    _show("Спільні складові", averages)
    _show("Деталі", details)


def haar_inverse(top, bottom, even_rows, odd_rows):
    print("\n=== Обернений Haar: відновлення перших рядків ===")
    _show("Верхня складова", top)
    _show("Нижня складова", bottom)
    _show("Відновлений парний рядок", even_rows)
    _show("Відновлений непарний рядок", odd_rows)


def summary(method, psnr, kept, total):
    quality = "inf" if np.isinf(psnr) else f"{psnr:.2f} dB"
    print(f"\n{method}: PSNR = {quality}, коефіцієнтів = {kept}/{total}\n")
