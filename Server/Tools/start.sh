#!/bin/bash

ulimit -c unlimited

 
nohup ./cmedia_rtc_server server.cfg log/ > cmedia_rtc_server.log 2>&1 &