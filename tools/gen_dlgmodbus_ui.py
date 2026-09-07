# -*- coding: utf-8 -*-
# 一次性生成 EdgeWidthDetection/ui/DlgModbus.ui（25 个 modbus 点位，3 个选项卡）
# 生成后即作为静态 UI 文件提交，本脚本不需重复运行

import xml.sax.saxutils as su

# 点位表（顺序固定，与 SetConfig.modbusAddressList / modbus.txt 行序一一对应）
# (索引, 默认名称, 默认地址, 类型, 读写)
POINTS = [
    (0,  "R_切刀点动速度",   1000, "float", "rw"),
    (1,  "R_设定拍照长度",   1002, "float", "rw"),
    (2,  "R_自动速度",       1026, "float", "rw"),
    (3,  "D_间隔袋数",       1028, "dint",  "rw"),
    (4,  "R_当前中心偏移值", 214,  "float", "ro"),
    (5,  "实际拍照值",       3000, "float", "ro"),
    (6,  "总偏移值",         3052, "float", "ro"),
    (7,  "编码器当前位置",   2000, "float", "ro"),
    (8,  "编码器当前速度",   3058, "dint",  "ro"),
    (9,  "R_切刀当前位置",   2004, "float", "ro"),
    (10, "切刀回原",         3000, "bool",  "rw"),
    (11, "切刀补偿开启",     1000, "bool",  "rw"),
    (12, "系统标志",         3004, "bool",  "ro"),
    (13, "启动",             3002, "bool",  "rw"),
    (14, "停止",             3003, "bool",  "rw"),
    (15, "R_白料长",         3040, "float", "ro"),
    (16, "R_d1袋长",         3030, "float", "ro"),
    (17, "切刀计算移动量",   3042, "float", "ro"),
    (18, "切刀实际移动量",   3060, "float", "ro"),
    (19, "R_编码器一圈脉冲数", 1006, "float", "rw"),
    (20, "R_编码器一圈距离",  1008, "float", "rw"),
    (21, "R_中心偏移最大值",  1030, "float", "rw"),
    (22, "R_中心偏移最小值",  1032, "float", "rw"),
    (23, "切刀移动最大值",    1034, "float", "rw"),
    (24, "切刀移动最小值",    1036, "float", "rw"),
]

TAB_RW   = [0, 1, 2, 3, 19, 20, 21, 22, 23, 24]          # 读写参数（数值）
TAB_RO   = [4, 5, 6, 7, 8, 9, 15, 16, 17, 18, 12]        # 只读数据（10数值 + 系统标志）
TAB_BOOL = [10, 11, 13, 14]                              # BOOL 控制（读写）

DIALOG_STYLE = """
QDialog {
    background-color: rgb(245, 245, 245);
}

QGroupBox {
    border: 1px solid #DDD;
    border-radius: 4px;
    font: bold 16px;
    color: #666;
    background-color: #F8F8F8;
    margin-top: 12px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 10px;
    padding: 0 5px;
}

QPushButton {
    padding: 6px 14px;
    border: 2px solid #CCC;
    border-radius: 4px;
    background-color: white;
    color: #444;
    font-size: 20px;
}

QPushButton:hover {
    border-color: #999;
    background-color: #F5F5F5;
}

QPushButton:pressed {
    border-color: #777;
    background-color: #EEE;
}

QLabel {
    color: #444444;
    font-size: 20px;
    background: transparent;
    padding: 2px 4px;
}

QTabWidget::pane {
    border: 1px solid #DDD;
    border-radius: 4px;
    background-color: #F8F8F8;
}

QTabBar::tab {
    padding: 8px 24px;
    font-size: 18px;
    color: #444;
    background: #EDEDED;
    border: 1px solid #DDD;
    border-bottom: none;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
}

QTabBar::tab:selected {
    background: #FFFFFF;
    font-weight: bold;
}
"""

# 当前值标签：数据展示样式（区别于名称标签）
VALUE_LABEL_STYLE = """QLabel {
    color: #2F4F4F;
    border: 1px solid #E0E0E0;
    background-color: #FFFFFF;
    padding: 6px 12px;
    border-radius: 4px;
}"""

# 地址按钮：浅蓝边框提示可点击修改
ADDR_BTN_STYLE = """QPushButton {
    padding: 6px 14px;
    border: 2px solid #87CEEB;
    border-radius: 4px;
    background-color: white;
    color: #2c3e50;
    font-size: 20px;
}

QPushButton:hover {
    border-color: #4682B4;
    background-color: #F0F8FF;
}

QPushButton:pressed {
    border-color: #2F4F4F;
    background-color: #E0F0F8;
}"""


def esc(s):
    return su.escape(str(s))


def prop_string(text, notr=False):
    attr = ' notr="true"' if notr else ''
    return (f'               <property name="styleSheet">\n'
            f'                <string{attr}>{text}</string>\n'
            f'               </property>')


def label(name, text, style=None, min_h=None):
    lines = [f'                 <widget class="QLabel" name="{name}">']
    if style:
        lines.append(prop_string(style.replace("\n", "&#10;"), notr=True))
    if min_h:
        lines.append(f'                 <property name="minimumSize">\n'
                     f'                  <size><width>0</width><height>{min_h}</height></size>\n'
                     f'                 </property>')
    lines.append(f'                 <property name="text">\n'
                 f'                  <string>{esc(text)}</string>\n'
                 f'                 </property>')
    lines.append('                 </widget>')
    return "\n".join(lines)


def button(name, text, style=None, min_w=120, min_h=40):
    lines = [f'                 <widget class="QPushButton" name="{name}">',
             f'                 <property name="minimumSize">\n'
             f'                  <size><width>{min_w}</width><height>{min_h}</height></size>\n'
             f'                 </property>']
    if style:
        lines.append(prop_string(style.replace("\n", "&#10;"), notr=True))
    lines.append(f'                 <property name="text">\n'
                 f'                  <string>{esc(text)}</string>\n'
                 f'                 </property>')
    lines.append('                 </widget>')
    return "\n".join(lines)


def header_row(cols):
    """列标题行"""
    items = []
    for col, text in enumerate(cols):
        items.append(f'              <item row="0" column="{col}">\n'
                     + label(f"lb_head_{col}_{len(cols)}", text) +
                     '\n              </item>')
    return "\n".join(items)


def grid_item(row, col, widget_xml):
    return f'              <item row="{row}" column="{col}">\n{widget_xml}\n              </item>'


def make_tab(tab_name, tab_title, point_indices, kind):
    """kind: 'rw' / 'ro' / 'bool'，生成一个选项卡页"""
    if kind == "rw":
        cols = ["点位名称", "地址(点击修改)", "当前值", "写入操作"]
    elif kind == "ro":
        cols = ["点位名称", "地址(点击修改)", "当前值"]
    else:
        cols = ["点位名称", "地址(点击修改)", "当前状态", "操作", ""]

    rows = [header_row(cols)]
    for i, pidx in enumerate(point_indices):
        _, name, addr, _typ, _acc = POINTS[pidx]
        r = i + 1
        rows.append(grid_item(r, 0, label(f"lb_name_{kind}_{i}", name, min_h=40)))
        rows.append(grid_item(r, 1, button(f"btn_addr_{kind}_{i}", addr, style=ADDR_BTN_STYLE)))
        rows.append(grid_item(r, 2, label(f"lb_value_{kind}_{i}", "--", style=VALUE_LABEL_STYLE, min_h=40)))
        if kind == "rw":
            rows.append(grid_item(r, 3, button(f"btn_write_{kind}_{i}", "写入")))
        elif kind == "bool":
            rows.append(grid_item(r, 3, button(f"btn_set1_{kind}_{i}", "置1", min_w=90)))
            rows.append(grid_item(r, 4, button(f"btn_set0_{kind}_{i}", "置0", min_w=90)))

    n = len(point_indices) + 1
    # 底部弹簧撑住，行内容靠上排
    rows.append(f'              <item row="{n}" column="0">\n'
                f'               <spacer name="verticalSpacer_{kind}">\n'
                f'                <property name="orientation"><enum>Qt::Vertical</enum></property>\n'
                f'                <property name="sizeHint" stdset="0"><size><width>20</width><height>40</height></size></property>\n'
                f'               </spacer>\n'
                f'              </item>')

    return f'''<widget class="QWidget" name="{tab_name}">
          <attribute name="title">
           <string>{esc(tab_title)}</string>
          </attribute>
          <layout class="QGridLayout" name="gridLayout_{kind}">
{chr(10).join(rows)}
          </layout>
         </widget>'''


def main():
    tabs = [
        ("tab_readWrite",   "读写参数",  TAB_RW,   "rw"),
        ("tab_readOnly",    "只读数据",  TAB_RO,   "ro"),
        ("tab_boolControl", "BOOL控制",  TAB_BOOL, "bool"),
    ]
    tabs_xml = "\n".join(
        "         " + make_tab(n, t, idxs, k).replace("\n", "\n         ")
        for n, t, idxs, k in tabs)

    title_style = """QLabel {
    color: #2c3e50;
    font-size: 24px;
    font-weight: bold;
    background: transparent;
    padding: 2px 4px;
}""".replace("\n", "&#10;")

    ui = f'''<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>DlgModbusClass</class>
 <widget class="QDialog" name="DlgModbusClass">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>1100</width>
    <height>800</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>Modbus 通讯设置</string>
  </property>
  <property name="styleSheet">
   <string notr="true">{DIALOG_STYLE.strip().replace(chr(10), "&#10;")}</string>
  </property>
  <layout class="QVBoxLayout" name="verticalLayout">
   <item>
    <layout class="QHBoxLayout" name="horizontalLayout_title">
     <item>
      <widget class="QLabel" name="lb_title">
       <property name="styleSheet">
        <string notr="true">{title_style}</string>
       </property>
       <property name="text">
        <string>Modbus 通讯设置</string>
       </property>
      </widget>
     </item>
     <item>
      <spacer name="horizontalSpacer_title">
       <property name="orientation">
        <enum>Qt::Horizontal</enum>
       </property>
       <property name="sizeHint" stdset="0">
        <size>
         <width>40</width>
         <height>20</height>
        </size>
       </property>
      </spacer>
     </item>
     <item>
      <widget class="QPushButton" name="btn_close">
       <property name="minimumSize">
        <size>
         <width>120</width>
         <height>45</height>
        </size>
       </property>
       <property name="text">
        <string>关闭</string>
       </property>
      </widget>
     </item>
    </layout>
   </item>
   <item>
    <widget class="QTabWidget" name="tabWidget_modbus">
     <property name="currentIndex">
      <number>0</number>
     </property>
{tabs_xml}
    </widget>
   </item>
  </layout>
 </widget>
 <resources/>
 <connections/>
</ui>
'''
    out = r"D:\Projects\EdgeWidthDetection\EdgeWidthDetection\ui\DlgModbus.ui"
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write(ui)
    print("written:", out, len(ui), "bytes")


if __name__ == "__main__":
    main()
