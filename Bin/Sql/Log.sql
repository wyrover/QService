CREATE TABLE  `logs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `serverid` int(11) DEFAULT '-1',
  `playerid` bigint(20) DEFAULT '-1',
  `logtype` int(11) DEFAULT '0',
  `logtime` varchar(32) DEFAULT '-1',
  `param0` int(11) DEFAULT '-1',
  `param1` int(11) DEFAULT '-1',
  `param2` int(11) DEFAULT '-1',
  `param3` int(11) DEFAULT '-1',
  `param4` int(11) DEFAULT '-1',
  `description` varchar(512) DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
