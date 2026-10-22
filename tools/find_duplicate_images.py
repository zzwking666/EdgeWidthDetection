# -*- coding: utf-8 -*-
"""
重名图片查找工具

功能：
  1. 遍历 A 文件夹（含子目录）下所有图片，记录文件名
  2. 遍历 B 文件夹（含子目录）下所有图片，找出与 A 重名的图片
  3. 将重名图片拷贝到 C 文件夹：
       C 根目录            —— B 侧的重名图片
       C/A侧同名对照/      —— A 侧同名图片（用于比对是否为同一张图）
       C/重名清单.txt      —— 本次查找的明细清单

打包为 exe（在本机已装 Python + PyInstaller 时，于仓库根目录执行）：
  pyinstaller --onefile --noconsole --name FindDuplicateImages tools/find_duplicate_images.py
生成的 exe 位于 dist/ 目录，拷贝到部署机可直接双击运行（无需 Python 环境）。
"""

import os
import shutil
import queue
import threading
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

# 识别的图片扩展名（小写）
IMAGE_EXTS = {".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff", ".webp"}


def collect_images(folder):
	"""递归收集 folder 下所有图片，返回 {文件名: [完整路径, ...]}"""
	result = {}
	for root, _dirs, files in os.walk(folder):
		for name in files:
			if os.path.splitext(name)[1].lower() in IMAGE_EXTS:
				result.setdefault(name, []).append(os.path.join(root, name))
	return result


def unique_target_path(target_dir, filename):
	"""目标目录下若已存在同名文件，则追加序号避免覆盖"""
	base, ext = os.path.splitext(filename)
	candidate = os.path.join(target_dir, filename)
	index = 2
	while os.path.exists(candidate):
		candidate = os.path.join(target_dir, f"{base} ({index}){ext}")
		index += 1
	return candidate


def find_and_copy(folder_a, folder_b, folder_c, log):
	"""核心流程：查找重名并拷贝。log 为日志回调函数"""
	log(f"正在扫描 A 文件夹：{folder_a}")
	images_a = collect_images(folder_a)
	log(f"A 文件夹共找到 {sum(len(v) for v in images_a.values())} 张图片（{len(images_a)} 个不同文件名）")

	log(f"正在扫描 B 文件夹：{folder_b}")
	images_b = collect_images(folder_b)
	log(f"B 文件夹共找到 {sum(len(v) for v in images_b.values())} 张图片")

	# B 中与 A 重名的文件名
	dup_names = sorted(set(images_b) & set(images_a))
	if not dup_names:
		log("未发现重名图片，结束。")
		return 0

	dir_a_side = os.path.join(folder_c, "A侧同名对照")
	os.makedirs(folder_c, exist_ok=True)
	os.makedirs(dir_a_side, exist_ok=True)

	copied = 0
	report_lines = [f"重名文件名共 {len(dup_names)} 个", ""]
	for name in dup_names:
		# 拷贝 B 侧所有该名图片到 C 根目录
		for src in images_b[name]:
			shutil.copy2(src, unique_target_path(folder_c, name))
			copied += 1
			report_lines.append(f"[B] {src}")
		# 拷贝 A 侧同名图片到对照目录，便于人工比对
		for src in images_a[name]:
			shutil.copy2(src, unique_target_path(dir_a_side, name))
			report_lines.append(f"[A] {src}")
		report_lines.append("")

	report_path = os.path.join(folder_c, "重名清单.txt")
	with open(report_path, "w", encoding="utf-8") as f:
		f.write("\n".join(report_lines))

	log(f"完成：共 {len(dup_names)} 个重名文件名，拷贝 B 侧图片 {copied} 张到 {folder_c}")
	log(f"A 侧同名图片已拷贝到 {dir_a_side}，清单见 {report_path}")
	return copied


class App(tk.Tk):
	def __init__(self):
		super().__init__()
		self.title("重名图片查找工具")
		self.resizable(False, False)

		self._log_queue = queue.Queue()
		self._build_ui()
		self.after(100, self._poll_log_queue)

	def _build_ui(self):
		frame = ttk.Frame(self, padding=12)
		frame.grid(row=0, column=0, sticky="nsew")

		self.var_a = tk.StringVar()
		self.var_b = tk.StringVar()
		self.var_c = tk.StringVar()

		rows = [
			("A 文件夹（参照）", self.var_a),
			("B 文件夹（查找重名）", self.var_b),
			("C 文件夹（结果输出）", self.var_c),
		]
		for row, (label, var) in enumerate(rows):
			ttk.Label(frame, text=label).grid(row=row, column=0, sticky="w", pady=4)
			ttk.Entry(frame, textvariable=var, width=56).grid(row=row, column=1, padx=6)
			ttk.Button(frame, text="浏览...",
				command=lambda v=var: self._choose_dir(v)).grid(row=row, column=2)

		self.btn_run = ttk.Button(frame, text="开始查找并拷贝", command=self._on_run)
		self.btn_run.grid(row=3, column=0, columnspan=3, pady=(10, 6), sticky="ew")

		self.txt_log = tk.Text(frame, width=80, height=16, state="disabled")
		self.txt_log.grid(row=4, column=0, columnspan=3, pady=(4, 0))

	def _choose_dir(self, var):
		path = filedialog.askdirectory()
		if path:
			var.set(path)

	def _on_run(self):
		folder_a, folder_b, folder_c = self.var_a.get(), self.var_b.get(), self.var_c.get()
		for label, path in (("A", folder_a), ("B", folder_b), ("C", folder_c)):
			if not path or not os.path.isdir(path):
				messagebox.showerror("错误", f"请选择有效的 {label} 文件夹")
				return
		if os.path.abspath(folder_c) in (os.path.abspath(folder_a), os.path.abspath(folder_b)):
			messagebox.showerror("错误", "C 文件夹不能与 A 或 B 相同")
			return

		self.btn_run.config(state="disabled")
		self._log("开始处理...")

		def worker():
			try:
				find_and_copy(folder_a, folder_b, folder_c, self._log)
				self._log_queue.put(("done", None))
			except Exception as exc:
				self._log_queue.put(("error", str(exc)))

		threading.Thread(target=worker, daemon=True).start()

	def _log(self, text):
		self._log_queue.put(("log", text))

	def _poll_log_queue(self):
		try:
			while True:
				kind, text = self._log_queue.get_nowait()
				if kind == "log":
					self.txt_log.config(state="normal")
					self.txt_log.insert("end", text + "\n")
					self.txt_log.see("end")
					self.txt_log.config(state="disabled")
				elif kind == "done":
					self.btn_run.config(state="normal")
					messagebox.showinfo("完成", "处理完成，请到 C 文件夹查看结果")
				elif kind == "error":
					self.btn_run.config(state="normal")
					messagebox.showerror("出错", text)
		except queue.Empty:
			pass
		self.after(100, self._poll_log_queue)


if __name__ == "__main__":
	App().mainloop()
