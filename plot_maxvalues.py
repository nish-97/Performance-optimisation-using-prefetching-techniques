import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the data from the CSV file into a DataFrame
workload = 'performance_max'
df = pd.read_csv(f'{workload}.csv')

dfs = []
# df_plot = df[['Benchmark','Speedup','L1D MPKI','L2C MPKI','Accuracy']]
# dff['Benchmark'] = pd.to_numeric(dff['Benchmark'], errors='coerce')
df_plot = df[['Benchmark','L1D MPKI','L2C MPKI']]
# if(len(df_plot) > 60):
# div = len(df_plot)//60
# print(div)
# for i in range(div):
# if(i < div-1):
# dfs.append(df_plot[i*60:(i+1)*60])
# else:
# dfs.append(df_plot[i*60:])
# else:
dfs.append(df_plot)

# Set the 'Trace' column as the index
for i,df in enumerate(dfs):
    df.set_index('Benchmark', inplace=True)
    ax = df.plot(kind='bar',width=0.3)
    # plt.ylim(0,0.075)
    plt.axis('auto')
    ax.axhline(y=1, color='red', linestyle='--')
    ax.set_ylabel('MPKI')
    ax.set_title('MPKI for prefetch degree,distance with max speedup')
    plt.xticks(rotation=90)

    plt.tight_layout()
# plt.savefig(f'{workload}_{i}.png')
# # Display the plot
# plt.show()
# for p in ax.patches:
#     x = p.get_x() + p.get_width() / 2.0
#     y = p.get_height()
# # Customize the position for positive and negative values
#     if y >= 0:
#         va = 'bottom'
#         annotation_position = 0.05
#     else:
#         va = 'top'
#         annotation_position = -6.5
# # ax.annotate(f'{y:.2f}', (x, y), xytext=(0, y), textcoords='offset points', ha='center', va=va, fontsize=7, color='black')
#     plt.text(x-0.005,y-0.0055,f"{y:.{1}f}",fontsize=8,ha='center',va='bottom')
# plt.text(-0.05,0.9,24,fontsize=10,ha='center',va='bottom')
# plt.text(0.9375,0.9,20,fontsize=10,ha='center',va='bottom')
# plt.text(1.925,0.9,78,fontsize=10,ha='center',va='bottom')
# plt.text(2.91255,0.9,83,fontsize=10,ha='center',va='bottom')
# plt.text(3.90005,0.9,49,fontsize=10,ha='center',va='bottom')

plt.savefig(f'{workload}_{i}.png')
# Display the plot
plt.show()