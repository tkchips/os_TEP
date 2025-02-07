import subprocess
import matplotlib.pyplot as plt
from concurrent.futures import ThreadPoolExecutor, as_completed
loops = 2500
# 存储结果的列表
i_values = []  # 存储 -i 的值
results = []   # 存储每次输出的结果

def run_command(i):
    command = [
        "python3", "x86.py",
        "-p", "looping-race-nolock.s",
        "-a", "bx=2000",
        "-t", "2",
        "-M", "2000",
        "-i", str(i),
        "-c"
    ]
    output = subprocess.run(command, capture_output=True, text=True)
    last_line = output.stdout.strip().split('\n')[-1]
    result_value = int(last_line.split()[0])
    return i, result_value



max_workers = 10  # 设置线程池的最大线程数
with ThreadPoolExecutor(max_workers=max_workers) as executor:
    futures = [executor.submit(run_command, i) for i in range(1, loops + 1)]  # 提交任务
    for future in as_completed(futures):  # 获取完成的任务结果
        i, result_value = future.result()
        i_values.append(i)
        results.append(result_value)
        if i % 100 == 0:
            print(f"Progress: {i}/1000")

sorted_results = sorted(zip(i_values, results), key=lambda x: x[0])

# 分离排序后的结果
sorted_i_values, sorted_results = zip(*sorted_results)
# 绘制折线图

plt.figure(figsize=(50, 6))  # 调整宽度为 15，高度为 6
plt.plot(sorted_i_values, sorted_results, marker='o', linestyle=' ', color='b')

step = 5  # 设定步长，即每隔几个点显示一个标签
plt.xticks(range(min(sorted_i_values), max(sorted_i_values)+1, step))
plt.title("Result Value vs -i Parameter")
plt.xlabel("-i Parameter Value")
plt.ylabel("Result Value (First Number in Last Line)")
plt.grid(True)

# 保存图片
plt.savefig("result_plot.png", dpi=300)  # 保存为 PNG 格式，分辨率为 300 DPI
print("图片已保存为 result_plot.png")

# 显示图片
plt.show()