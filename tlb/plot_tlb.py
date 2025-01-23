import numpy as np
import matplotlib.pyplot as plt

# 读取数据
pages = []
times = []
with open("final_results.txt", "r") as f:
    for line in f:
        p, t = line.strip().split()
        pages.append(int(p))
        times.append(float(t))

pages = np.array(pages)
times = np.array(times)

# 检测跃升点（核心逻辑）
def find_jumps(times, threshold=10.0, min_gap=2):
    """
    参数:
        threshold: 时间跃升阈值 (ns)
        min_gap: 相邻跃升点最小间隔 (避免重复检测噪声)
    返回:
        jumps: 跃升点索引列表 (对应pages数组)
    """
    diffs = np.diff(times)
    jumps = []
    prev_idx = -min_gap  # 初始化确保第一个检测有效
    
    for i in range(len(diffs)):
        if diffs[i] > threshold:
            if (i - prev_idx) >= min_gap:  # 避免连续检测
                jumps.append(i + 1)  # diff索引i对应原数组i+1
                prev_idx = i
    return jumps

# 自动检测跃升点
threshold = 10.0  # 根据实际数据调整
jumps = find_jumps(times, threshold=threshold)

# 推测 TLB 层级标签 (根据跃升点数量自动生成)
tlb_labels = ["L1 TLB", "L2 TLB", "L3 TLB"]  # 最多支持三级
annotations = []
for i, idx in enumerate(jumps):
    if i < len(tlb_labels):
        label = f"{tlb_labels[i]} Size: {pages[idx-1]}"  # 跃升点前的页数是容量
        annotations.append((pages[idx], times[idx], label))

# 绘图
plt.figure(figsize=(12, 7))
plt.plot(pages, times, marker='o', linestyle='-', markersize=8, linewidth=1.5, color='#2c7bb6')

# 标注跃升点
colors = ['#d7191c', '#fdae61', '#abd9e9']  # 红/橙/蓝 用于多级标注
for i, (x, y, label) in enumerate(annotations):
    plt.annotate(
        label,
        xy=(x, y),
        xytext=(x * 1.2, y + 15 + i*20),  # 动态调整标注位置
        arrowprops=dict(arrowstyle="->", color=colors[i], lw=1.5),
        fontsize=12,
        color=colors[i]
    )
    plt.axvline(x=x, color=colors[i], linestyle='--', alpha=0.5)  # 添加竖线

# 图饰
plt.xscale("log", base=2)
plt.xticks(pages, labels=pages, rotation=45)
plt.xlabel("Number of Pages", fontsize=14)
plt.ylabel("Average Access Time per Page (ns)", fontsize=14)
plt.title("TLB Hierarchy Detection with Automatic Jump Marking", fontsize=16)
plt.grid(True, which="both", linestyle="--", alpha=0.4)
plt.tight_layout()

# 保存和显示
plt.savefig("tlb_hierarchy.png", dpi=300, bbox_inches='tight')
plt.show()