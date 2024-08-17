from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer

def start_ftp_server():
    # 创建一个授权器实例
    authorizer = DummyAuthorizer()

    # 添加用户权限：用户名为 'user', 密码为 '12345', 主目录为当前目录，权限为读写
    authorizer.add_user("user", "12345", ".", perm="elradfmw")

    # 创建一个FTP处理程序，并将其授权器设置为刚才创建的授权器
    handler = FTPHandler
    handler.authorizer = authorizer

    # 创建FTP服务器实例，监听所有IP地址的21端口
    server = FTPServer(("0.0.0.0", 21), handler)

    # 启动FTP服务器
    print("Starting FTP server...")
    server.serve_forever()

if __name__ == "__main__":
    start_ftp_server()
