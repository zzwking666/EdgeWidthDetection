# -*- coding: utf-8 -*-
"""根据新版《通讯地址.xlsx》生成 modbus.csv（页签,名称,地址,类型,协议地址,读写）"""
import sys, io, csv
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
import openpyxl

wb = openpyxl.load_workbook(r'C:\Users\zfkj4090\Desktop\通讯地址.xlsx', data_only=True)

rows = []
for ws in wb.worksheets:
    tab = ws.title.strip()
    writable = '只读' if '只读' in tab else '读写'
    for i, row in enumerate(ws.iter_rows(values_only=True)):
        name, addr, dtype, proto = (list(row) + [None] * 4)[:4]
        if i == 0:
            continue  # 首行是标题行（A 列为 sheet 名）
        if name is None or str(name).strip() == '':
            continue
        rows.append([tab, str(name).strip(), str(addr).strip(), str(dtype).strip(),
                     str(int(proto)), writable])

out = r'D:\zfkjData\EdgeWidthDetection\config\modbus.csv'
with open(out, 'w', encoding='utf-8-sig', newline='') as f:
    w = csv.writer(f)
    w.writerow(['页签', '名称', '地址', '类型', '协议地址', '读写'])
    w.writerows(rows)

print(f'共写入 {len(rows)} 行 -> {out}')
for r in rows:
    print(','.join(r))
