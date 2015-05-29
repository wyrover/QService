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

        
