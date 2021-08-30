import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import time

def getMinKey(dict):
    minkey = 200000 #max ITCH 5.0 price
    for key in dict:
        if key < minkey:
            minkey = key
    return minkey

def getMaxKey(dict):
    maxkey = 0
    for key in dict:
        if key > maxkey:
            maxkey = key
    return maxkey

def checkLevels(dict, price, shares, num_levels, side):
    if price in dict:
        dict[price] += shares
        if dict[price] <= 0:
            del dict[price]
            return False
        return True
    elif len(dict) < num_levels:
        if shares > 0:
            dict[price] = shares
            return True
        return False
    elif side == 'B':
        minkey = getMinKey(dict)
        if price > minkey and shares >= 0:
            del dict[minkey]
            dict[price] = shares
            return True
    elif side == 'S':
        maxkey = getMaxKey(dict)
        if price < maxkey and shares >= 0:
            del dict[maxkey]
            dict[price] = shares
            return True
    return False

def readFile(plotFunction, num_levels): #faster than pandas when iterating over every row of data
    hl, = plt.plot([], [])
    with open('../build/AAPL') as file:
        levels = {
            'bids': {}, 
            'asks': {}
        }   
        levels_change = False
        for line in file:
            temp = [n for n in line.strip().split(',')]
            if temp[0] == 'B':
                levels_change = checkLevels(levels['bids'], float(temp[2]) / 10000, int(temp[1]), num_levels, temp[0])
            else:
                levels_change = checkLevels(levels['asks'], float(temp[2]) / 10000, int(temp[1]), num_levels, temp[0])
            if levels_change:
               plotFunction(levels)
        print(levels)

class dynamicBarPlot():
    def initBar(self):
        self.figure, self.ax = plt.subplots()
        self.ax.bar([],[])
        self.ax.set_xlim(0, 100)
        self.ax.grid()

    def getBounds(self, levels):
        ask_prices = np.array(list(levels['asks']))
        bid_prices = np.array(list(levels['bids']))
        lower_x_bound = np.min(bid_prices) if bid_prices.size != 0 else np.min(ask_prices)
        upper_x_bound = np.max(ask_prices) if ask_prices.size != 0 else np.max(bid_prices)
        graph_interval = upper_x_bound - lower_x_bound
        ten_percent = graph_interval / 10
        upper_x_bound += ten_percent
        lower_x_bound -= ten_percent
        if lower_x_bound == upper_x_bound:
            upper_x_bound += 1
        return lower_x_bound, upper_x_bound

    def getWidth(self, prices):
        if len(prices) < 2:
            return 0.05
        print(len(prices))
        #print(share_nums)
        width = np.diff(np.unique(prices)).min() / 2
        return np.diff(np.unique(prices)).min() / 2

    def onRunning(self, levels):
        ask_sizes = list(levels['asks'].values())
        bid_sizes = list(levels['bids'].values())
        ask_prices = list(levels['asks'])
        bid_prices = list(levels['bids'])
        xdata = np.append(
            list(levels['asks']),
            list(levels['bids'])
        )
        self.ax.clear()
        width = self.getWidth(xdata)
        if len(ask_sizes) != 0:
            self.ax.bar(ask_prices, ask_sizes, width=width, color='red')
        if len(bid_sizes) != 0:
            self.ax.bar(bid_prices, bid_sizes, width=width, color='green')
        minx, maxx = self.getBounds(levels)
        self.ax.set_xlim(minx, maxx)
        self.figure.canvas.draw()
        self.figure.canvas.flush_events()
        time.sleep(0.01)
    
    def __call__(self):
        self.initBar()
        readFile(self.onRunning, 5)

def main():
    plt.ion()
    bar = dynamicBarPlot()
    bar()
if __name__ == "__main__":
    main()
