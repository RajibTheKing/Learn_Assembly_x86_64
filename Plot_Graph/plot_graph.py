import matplotlib.pyplot as plt
  
# Statistical Data 
x =     [80007,     90009,     100011,      110013,     1000003,    1100007,    1200009,    10000004,   11000003,   12000009]
y1 =    [802901,    967873,    1108531,     1189427,    3164134,    3528646,    3774193,    31765328,   34791641,   38439044]
y2 =    [33835,     40611,     45164,       49766,      132179,     143818,     156841,     1416264,    1584822,    1745398]


plt.plot(x, y1, marker='o', label = "C++ Naive Solution")
  
plt.plot(x, y2, marker='o', label = "Assembly Solution")
  
# naming the x axis
plt.xlabel('Number of Characters')
# naming the y axis
plt.ylabel('Time (Nanoseconds)')
# giving a title to my graph
plt.title('Case Insensitive Compare execution time')
  
# show a legend on the plot
plt.legend()
  
# function to show the plot
plt.show()
