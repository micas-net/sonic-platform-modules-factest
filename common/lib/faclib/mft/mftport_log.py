#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
u'''生测PORT log组件'''


import logging
import logging.handlers
import inspect

class PortLogger:
    MAX_MESSAGE_LENGTH = 1024

    def __init__(self, name, level):
        self.logger = logging.getLogger(name)
        self.set_level(level)

        # 创建 SysLogHandler
        syslog_handler = logging.handlers.SysLogHandler(address='/dev/log')
        syslog_handler.setLevel(logging.DEBUG)

        # 创建格式化器并添加到处理器
        # syslog 已经自带日志登记和时间了
        # formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s [%(user_funcName)s:%(user_lineno)d]: %(message)s')
        formatter = logging.Formatter('%(name)s [%(user_funcName)s:%(user_lineno)d][%(process)d]: %(message)s')
        syslog_handler.setFormatter(formatter)

        # 将处理器添加到日志器
        self.logger.addHandler(syslog_handler)

    def set_level(self, level):
        """设置日志级别"""
        if level == "debug":
            self.logger.setLevel(logging.DEBUG)
        elif level == "info":
            self.logger.setLevel(logging.INFO)
        elif level == "warning":
            self.logger.setLevel(logging.WARNING)
        elif level == "error":
            self.logger.setLevel(logging.ERROR)
        elif level == "critical":
            self.logger.setLevel(logging.CRITICAL)

    def log_message(self, message, level):
        # 获取调用栈并获取更上一级的帧
        stack = inspect.stack()
        caller_frame = stack[2][0]  # 这里使用索引 2 来获取调用 log_message 的函数的帧
        extra = {
            'user_funcName': caller_frame.f_code.co_name,   # 获取调用函数的名字
            'user_lineno': caller_frame.f_lineno,      # 获取行号
        }
        for line in message.splitlines():
            self.logger.log(level, line, extra=extra)
        # if len(message) > self.MAX_MESSAGE_LENGTH:
        #     # 按行拆分并记录
        #     for i in range(0, len(message), self.MAX_MESSAGE_LENGTH):
        #         self.logger.log(level, message[i:i + self.MAX_MESSAGE_LENGTH], extra=extra)
        # else:
        #     self.logger.log(level, message, extra=extra)

    def debug(self, message):
        self.log_message(message, logging.DEBUG)

    def info(self, message):
        self.log_message(message, logging.INFO)

    def warning(self, message):
        self.log_message(message, logging.WARNING)

    def error(self, message):
        self.log_message(message, logging.ERROR)

    def critical(self, message):
        self.log_message(message, logging.CRITICAL)
