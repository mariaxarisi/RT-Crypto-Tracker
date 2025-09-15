# RT-Crypto-Tracker 

A real-time cryptocurrency analysis system designed to run efficiently on low-power microcomputers (e.g., Raspberry Pi). It collects trading data from the **OKX WebSocket API** for multiple crypto pairs and computes statistical metrics such as **moving averages** and **Pearson correlations**.

--- 

## Features 

- Collects live trade data for 8 crypto pairs: 
    - `BTC-USDT`, `ETH-USDT`, `ADA-USDT`, `SOL-USDT`,  
    `LTC-USDT`, `DOGE-USDT`, `XRP-USDT`, `BNB-USDT`
- Multithreaded architecture: 
    - **WebSocket thread** – manages live data stream and reconnections 
    - **Parser thread** – extracts trades (timestamp, price, size) 
    - **Moving Average thread** – computes 15-min volume-weighted averages 
    - **Pearson thread** – calculates correlations across assets 
    - **CPU monitoring thread** – tracks performance 
- Efficient synchronization with mutexes and condition variables
- Designed for long, low-CPU continuous operation on embedded hardware

---

## Compilation and Deployment

### Native Compilation (Linux Host)

1. **Install Dependencies**     
    Ensure you have the following libraries on your system:     
        - `libjansson-dev`  
        - `libwebsockets-dev`   
        - `build-essential` (for `gcc`, `make` and `pthread`)

    ``` bash
    sudo apt-get update
    sudo apt-get install build-essential libjansson-dev libwebsockets-dev
    ```
2. **Compile the Project**  
    Run the following command in the project root:
    ``` bash
    make
    ```
3. **Run the Executable**   
    After compilation, execute the program:
    ``` bash
    ./crypto
    ```

### Cross-Compilation for Raspberry Pi

1. **Prepare  the Raspberry Pi**       
    On your Raspberry Pi, install the required libraries:
    ``` bash
    sudo apt-get update
    sudo apt-get install libjansson-dev libwebsockets-dev libssl-dev libpthread
    ```
2. **Create a Sysroot on your host Machine**    
        - Copy the `lib`, `usr/lib` and `usr/include` directories from your Raspberry Pi to your host machine.  
        - Place them under `~/rpi-sysroot`.
    ``` bash
    rsync -avz pi@<raspberry_ip>:/lib ~/rpi-sysroot/
    rsync -avz pi@<raspberry_ip>:/usr/lib ~/rpi-sysroot/usr/
    rsync -avz pi@<raspberry_ip>:/usr/include ~/rpi-sysroot/usr/
    ```
3. **Install the ARM Cross-Compiler**   
    On your host machine:
    ``` bash
    sudo apt-get install gcc-arm-linux-gnueabihf
    ```
4. **Cross-Compile the Project**    
    In your project root, run:
    ``` bash
    make pi
    ```
    This will produce the `crypto-pi` binary for the Raspberry Pi.
5. **Transfer the Binary to the Raspberry Pi**      
    ``` bash
    scp crypto-pi pi@<raspberry_ip>:~
    ```
6. **Run the Executable**       
    On the Raspberry Pi:
    ``` bash
    ./crypto-pi
    ```

## Results
- Sustained operation for multiple days on Raspberry Pi Zero 2 W  
- Average CPU usage ~5%, with short peaks during reconnections  
- Pearson correlation calculations completed in <0.12s consistently  
- Logs include Pearson correlations, thread delays, and CPU usage statistics  

## Documentation

See [RT_Crypto_Tracker.pdf](docs/RT_Crypto_Tracker.pdf) for a detailed technical report.

## Author

Maria Charisi   
Aristotle University of Thessaloniki