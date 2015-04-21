QService
========

QService服务器支持http、 tcp 、websocket协议；数据库支持sqlite3、mysql、redis。  
所有业务均可用Lua脚本开发，已经用于手游服务器端开发。

一、编译QService  
windows使用vs2010或者更高版本；  
linux使用mklib.sh编译出依赖库，然后再mk.sh。  

二、配置文件  
    config.xml 配置服务器启动参数  

三、Lua文件  
    1、Main.lua  
        Main.lua为主lua文件，执行一些必须的事件，如消息处理分发、连接断开等。  
    2、RegFuncs.lua  
        RegFuncs.lua实现事件注册，回调。  
    3、Utile.lua   
        一些常用函数  
    4、RegSV.lua  
       服务器注册  
    5、Protocol.lua  
        操作码、请求码   
    6、Macros.lua  
        宏、枚举  
    7、ErrorCodes.lua  
        错误码  
    8、Debug.lua  
        debug   
    9、DBManager.lua  
        数据库  
    10、其他Lua文件  
四、事件  
    1、事件定义  
        事件的枚举在Macros.lua中的GameEvent，用户可以自定义事件如玩家升级等。  
    2、事件执行  
        如玩家升级，在玩家等级提升后执行onGameEvent(GameEvent.LevelUp, objPlayer)  
    3、事件注册  
        如玩家升级,regGameEvent(GameEvent.LevelUp, taskOnLevelUp) taskOnLevelUp为玩家  
    等级提升以后任务处理函数。  
  
五、Lua中使用的c++函数  
    向lua导出的函数参看Reg2Lua.cpp 中的void Register(void)函数  
    1、函数  
        Q_GetModulPath 获取QService执行文件所在路径  
        Q_GetPathSeparator 获取路径分隔符("/"或"\")  
        Q_LOG 文本日志  
        Q_DBLOG 数据库日志  
        getServerID 获取配置表中的服务器ID  
        getServerType 获取配置表中的服务器类型  
    2、类 这里主要介绍session和session管理  
        CSession session类每个连接对应一session 在Lua中不允许实例化  
            getSessionID 获取session ID号  
            setID  关联的外部ID 如玩家ID,主要通过该ID与玩家对应  
            getID  获取关联的外部ID  
            setCheckID 设置校验ID  
            getCheckID 获取校验ID  
            setPing  设置ping  
            setAccount 设置关联的账号   
            getAccount 获取关联的账号  
            setStatus 设置状态 值为Macros.lua中的SessionStatus枚举  
            getStatus 获取状态    
            getType 获取session类型 值为Macros.lua中的SessionType枚举   
        CSessionManager session管理 在Lua中不允许实例化 对应于Main.lua中的g_objSessionManager变量  
            closeLinkByID 关闭连接根据session ID   
            closeCurLink 关闭当前session连接   
            getCurSession 获取当前session对象 CSession类型  在网络可读和断开的时候有值其他时候为nil  
            getSessionByID 根据session ID获取session对象  
            sendToCur 向当前session发送消息  
            sendToByID 向指定ID的session发送消息   
            checkPing ping检查，参数为超时时间   
            getTimer 获取计时器触发时间间隔   
            getCount 获取计时器触发的次数    
            confirmStop 确认退出服务器   
            getLinkOtherID 根据名称获取链接到其他服务器的ID             
           
水平有限，如发现bug或有什么意见、建议请联系我，大家一起完善这个框架。email:200309129@163.com。
