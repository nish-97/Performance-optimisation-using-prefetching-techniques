import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the data from the CSV file into a DataFrame
workload = 'performance_stream_stride'
df = pd.read_csv(f'{workload}.csv')

dfs = []
#dff = df[['Benchmark','Speedup_10','Speedup_9','Speedup_8','Speedup_7','Speedup_6','Speedup_5','Speedup_4','Speedup_3','Speedup_2']]
dff = df[['Benchmark','Speedup']]

df_plot=dff

# if(len(df_plot) > 60):
#     div = len(df_plot)//60
#     print(div)
#     for i in range(div):
#         if(i < div-1):
#             dfs.append(df_plot[i*60:(i+1)*60])
#         else:
#             dfs.append(df_plot[i*60:])
# else:
dfs.append(df_plot)



# Set the 'Trace' column as the index
for i,df in enumerate(dfs):
    df.set_index('Benchmark', inplace=True)
    ax = df.plot(kind='bar',width=0.8)
    plt.ylim(0,2)
    # plt.axis('auto')
    ax.axhline(y=1, color='red', linestyle='--', label='y=0')
    ax.set_ylabel('Speedups')
    ax.set_title('Speedups for Stream plus Stride Prefetcher')
    plt.xticks(rotation=90)
    # plt.legend(title='Legend')
    plt.tight_layout()
    # plt.savefig(f'{workload}_{i}.png')
    # # Display the plot
    # plt.show()

    bar_width = 0.5


# for p in ax.patches:
#         x = p.get_x() + p.get_width() / 2.0
#         y = p.get_height()
        
#         # Customize the position for positive and negative values
#         if y >= 0:
#             va = 'bottom'
#         #     annotation_position = 0.05
#         else:
#             va = 'top'
#             # annotation_position = -35.5
        
#         # ax.annotate(f'{y:.2f}', (x, y), xytext=(0, y), textcoords='offset points', ha='center', va=va, fontsize=5, color='black')
#         plt.text(x-0.005,y+0.055,f"{y:.{1}g}",fontsize=7,ha='center',va='bottom')

plt.savefig(f'{workload}_{i}.png')
    
    # Display the plot
plt.show()
