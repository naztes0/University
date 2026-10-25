import tkinter as tk
from tkinter import filedialog, messagebox

import numpy as np
from PIL import Image, ImageTk

from compression import compress_dct, compress_dwt


class App:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("DCT vs DWT")
        self.root.geometry("1120x520")
        self.source = None
        self.previews = [None, None, None]

        controls = tk.Frame(root)
        controls.pack(fill="x", padx=16, pady=12)

        tk.Button(controls, text="Відкрити зображення", command=self.open_image).pack(side="left")
        tk.Label(controls, text="Залишити коефіцієнтів:").pack(side="left", padx=(20, 6))

        self.percent = tk.IntVar(value=20)
        tk.Scale(
            controls,
            from_=1,
            to=100,
            orient="horizontal",
            variable=self.percent,
            length=260,
        ).pack(side="left")
        tk.Button(controls, text="Стиснути", command=self.compress).pack(side="left", padx=10)

        root.bind("<Left>", lambda _: self.adjust_percent(-1))
        root.bind("<Right>", lambda _: self.adjust_percent(1))
        root.bind("<Return>", lambda _: self.compress())

        self.panels = []
        content = tk.Frame(root)
        content.pack(fill="both", expand=True, padx=10)

        for title in ("Оригінал", "DCT", "DWT (Хаар)"):
            panel = tk.Frame(content)
            panel.pack(side="left", fill="both", expand=True, padx=6)
            tk.Label(panel, text=title, font=("Arial", 13, "bold")).pack(pady=5)
            image_label = tk.Label(panel, bg="#eeeeee")
            image_label.pack(fill="both", expand=True)
            info_label = tk.Label(panel, text="", justify="left")
            info_label.pack(pady=8)
            self.panels.append((image_label, info_label))

    def adjust_percent(self, step: int):
        value = max(1, min(100, self.percent.get() + step))
        self.percent.set(value)

    def open_image(self):
        path = filedialog.askopenfilename(
            filetypes=[("Images", "*.png *.jpg *.jpeg *.gif")]
        )
        if not path:
            return

        image = Image.open(path).convert("L")
        self.source = np.array(image)
        self.show(0, self.source)
        self.panels[0][1].config(text=f"{image.width} × {image.height} px")
        self.compress()

    def compress(self):
        if self.source is None:
            messagebox.showinfo("DCT vs DWT", "Спочатку відкрийте зображення.")
            return

        percent = self.percent.get()
        dct = compress_dct(self.source, percent)
        dwt = compress_dwt(self.source, percent)

        self.show(1, dct.image)
        self.show(2, dwt.image)
        self.panels[1][1].config(text=self.describe(dct))
        self.panels[2][1].config(text=self.describe(dwt))

    @staticmethod
    def describe(result):
        quality = "∞" if np.isinf(result.psnr) else f"{result.psnr:.2f}"
        percent = result.kept / result.total * 100
        return f"PSNR: {quality} dB\nЗбережено: {result.kept:,} / {result.total:,} ({percent:.1f}%)"

    def show(self, index: int, array: np.ndarray):
        image = Image.fromarray(array)
        image.thumbnail((350, 350), Image.Resampling.LANCZOS)
        preview = ImageTk.PhotoImage(image)
        self.previews[index] = preview
        self.panels[index][0].config(image=preview)


if __name__ == "__main__":
    root = tk.Tk()
    App(root)
    root.mainloop()
