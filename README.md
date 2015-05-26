QService
========

QService服务器支持http、 tcp 、websocket协议；数据库支持sqlite3、mysql、mariadb、redis。  
所有业务均可用Lua脚本开发，已经用于手游服务器端开发。

##一、编译QService  
* windows使用vs2010或者更高版本；  
* linux使用mklib.sh编译出依赖库，然后再mk.sh。  

##二、配置文件  
* config.xml 配置服务器启动参数  

##三、Lua文件  
* Main.lua  
    Main.lua为主lua文件，执行一些必须的事件，如消息处理分发、连接断开等。  
* RegFuncs.lua  
    RegFuncs.lua实现事件注册，回调。  
* Utile.lua   
    一些常用函数  
* RegSV.lua  
    服务器注册  
* Protocol.lua  
	操作码、请求码   
* Macros.lua  
	宏、枚举  
* ErrorCodes.lua  
	错误码  
* Debug.lua  
	debug   
* DBManager.lua  
	数据库  
* TimeWheel.lua   
	时间轮   
* 其他Lua文件  

##四、事件  
* 事件定义  
	事件的枚举在Macros.lua中的GameEvent，用户可以自定义事件如玩家升级等。  
* 事件执行  
	如玩家升级，在玩家等级提升后执行onGameEvent(GameEvent.LevelUp, objPlayer)  
* 事件注册  
	如玩家升级,regGameEvent(GameEvent.LevelUp, taskOnLevelUp) taskOnLevelUp为玩家  
等级提升以后任务处理函数。  
  
##五、Lua中使用的c++函数  
向lua导出的函数参看Reg2Lua.cpp 中的void Register(void)函数   

* 函数    
	Q_GetModulPath 获取QService执行文件所在路径  
	Q_GetPathSeparator 获取路径分隔符("/"或"\")  
	Q_LOG 文本日志  
	Q_DBLOG 数据库日志  
	getServerID 获取配置表中的服务器ID  
	getServerNam 获取配置表中的服务器名称    

* 类 这里主要介绍session、session管理、CLuaBinary    

	CLuaBinary 二进制操作类  在Lua中不允许实例化   
	
		getLens 获取要解析的数据总长度    
		reSetWrite 重置写标记，需要写入新数据前先调用次    
		skipRead 读 跳过多少字节（结构体内存对齐使用）    
		skipWrite 写  跳过多少字节（结构体内存对齐使用）  

		setSint8 写入1个char      
		getSint8 读取1个char  
		setUint8 写入1个unsigned char      
		getUint8 读取1个unsigned char    
		
		setBool 写入bool    
		getBool 读取bool    
        
		setSint16 写入 short   
		getSint16 读取 short   
		setUint16 写入 unsigned short    
		getUint16 读取 unsigned short   
        
		setSint32 写入 int   
		getSint32 读取 int  
		setUint32 写入 unsigned int  
		getUint32 读取 unsigned int  
        
		setSint64 写入 int64_t    
		getSint64 读取 int64_t    
		setUint64 写入 uint64_t    
		getUint64 读取 uint64_t  
        
		setDouble 写入 double    
		getDouble 读取 double    
        
		setFloat 写入 float    
		getFloat 读取 float    
        
		setString 写入 string 保证以/0结束     
		getString 读取 string 
        
		setByte 写入 二进制    
		getByte 读取 二进制    
        
		setStruct 写入 结构体    
		getStruct 读取 结构体   
        
		getBuffer 获取写入的数据    

	CSession session类每个连接对应一session 在Lua中不允许实例化  
	
		getSessionID 获取session ID号  
        
		setExterID  关联的外部ID 如玩家ID,主要通过该ID与玩家对应  
		getExterID  获取关联的外部ID  
        
		setCheckID 设置校验ID  
		getCheckID 获取校验ID  
        
		setIDCard 设置session全局唯一标识  
		getIDCard 获取session全局唯一标识  
        
		setPing  设置ping   
		getPing  获取ping  
        
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
		sendBToCur 向当前session发送CLuaBinary中的消息  
		sendBToByID 向指定ID的session发送CLuaBinary中的消息         
        
		getTimer 获取计时器触发时间间隔   
		getCount 获取计时器触发的次数     
        
		getSessionSize 获取链接总数   
		confirmStop 确认退出服务器  
        
		addLinkOther 增加连接到其他服务器    
		getLinkOtherID 根据名称获取链接到其他服务器的ID             
        