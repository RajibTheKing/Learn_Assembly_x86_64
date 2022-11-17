import matplotlib.pyplot as plt

class GraphGenerator:
    def __init__(self):
        self.fileName = "/home/rajib/Desktop/Learn_Assembly(x86_64)/Plot_Graph/execution_time_data.txt"
        self.fileNameToWrite = "/home/rajib/Desktop/Learn_Assembly(x86_64)/Plot_Graph/table.md"
        self.filePtr = open(self.fileName, "r")
        self.filePtr2 = open(self.fileNameToWrite, "w+")
        self.readFromFile()
        self.generateGraph()
        self.generateMarkdownTable()
    
    def readFromFile(self):
        self.numberOfSolution = int(self.filePtr.readline())
        self.solutions = []
        for i in range(0, self.numberOfSolution):
            sol_name = self.filePtr.readline()
            self.solutions.append(sol_name[:-1])
            
        print(self.solutions)

        self.testCase = int(self.filePtr.readline())
        self.lengths = []

        #initialize execution_time with empty array
        self.execution_time = []
        for i in range(0, self.numberOfSolution):
            self.execution_time.append([])

        for i in range(0, self.testCase):
            len = int(self.filePtr.readline())
            self.lengths.append(len)
            line = self.filePtr.readline()
            times = [int(x) for x in line.split()]
            for j in range(0, self.numberOfSolution):
                self.execution_time[j].append(times[j])

        #print(self.execution_time)

    def generateGraph(self):
        # Statistical Data 

        print(self.lengths)
        x =  self.lengths

        for i in range(1, self.numberOfSolution):
            y = self.execution_time[i]
            plt.plot(x, y, marker='.', label = self.solutions[i])

        # y1 = self.execution_time[0]
        # y2 = self.execution_time[1]


        # plt.plot(x, y1, marker='o', label = self.solutions[0])
        
        # plt.plot(x, y2, marker='x', label = self.solutions[1])
        
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

    def generateMarkdownTable(self):
        #Header row of the table
        line = "| Number of Characters |"
        for i in range(0, self.numberOfSolution):
            line = line + (" " + self.solutions[i] + " |")
        line = line + "\r\n"
        self.filePtr2.write(line)

        line = "| ---------- |"
        for i in range(0, self.numberOfSolution):
            line = line + (" ---------- |")
        line = line + "\r\n"
        self.filePtr2.write(line)

        for i in range(0, self.testCase):
            line = "| " + str(self.lengths[i]) + " |"
            for j in range(0, self.numberOfSolution):
                line = line + str(self.execution_time[j][i]) + " |"
            line = line + "\r\n"
            self.filePtr2.write(line)

        self.filePtr2.close()


def main():
    print("Inside Main")
    graphGenerator = GraphGenerator()


if __name__ == "__main__":
    main()
