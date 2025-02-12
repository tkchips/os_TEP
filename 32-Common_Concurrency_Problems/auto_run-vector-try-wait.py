import subprocess
import matplotlib.pyplot as plt
import numpy as np

# 定义参数范围
num_threads_list = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32]
loops_list = [1000]

# 存储结果的二维数组
results = []

# 遍历不同的线程数和循环次数
for loops in loops_list:
    row = []
    print(f"Running with loops={loops}")
    for num_threads in num_threads_list:
        # 构造命令
        print(f"Running with loops={loops} and num_threads={num_threads}")
        command = f"./vector-try-wait -t -n {num_threads} -l {loops} -d"
        
        try:
            # 执行命令并捕获输出
            output = subprocess.check_output(command, shell=True, text=True)
            
            # 解析时间值
            time_line = [line for line in output.splitlines() if "Retries:" in line][0]
            time_value = float(time_line.split(" ")[1])
            row.append(time_value)
        except Exception as e:
            print(f"Error running command: {command}")
            print(e)
            row.append(None)  # 如果出错，记录为 None
    
    results.append(row)

# 打印结果表格
print("Results Table:")
header = "   ".join([f"{n:<5}" for n in ["-l/-n"] + num_threads_list])
print(header)
for i, loops in enumerate(loops_list):
    row_str = f"{loops:<7}" + "   ".join([f"{t:.2f}" if t is not None else "N/A" for t in results[i]])
    print(row_str)

# 绘制趋势图
x = np.array(num_threads_list)
plt.figure(figsize=(10, 6))

for i, loops in enumerate(loops_list):
    y = np.array(results[i])
    plt.plot(x, y, label=f"Loops = {loops}", marker='o')

# 设置图表属性
plt.title("Execution Time vs Number of Threads")
plt.xlabel("Number of Threads (-n)")
plt.ylabel("Execution Time (seconds)")
plt.xticks(num_threads_list)
plt.legend()
plt.grid(True)

# 显示图表
plt.show()

output_image_path = "vector_try_wait.png"
plt.savefig(output_image_path, dpi=300, bbox_inches='tight')  # 保存为高分辨率 PNG 文件
print(f"Image saved to {output_image_path}")