# ITCH 5.0 Data Feed

A low latency data feed and order book reconstructor for NASDAQ's TotalView-ITCH 5.0 outbound protocol. 

## Messages Supported

Currently, all messages that modify order books are supported:

* Add Order No MPID
* Add Order MPID
* Order Executed
* Order Executed with Price
* Order Cancelled
* Order Deleted
* Order Replaced

## Features Planned:

* Proper low latency logging capabilities, easily interfacable with any potential usage.
