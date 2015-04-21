CREATE TABLE `log` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `logtype` int(11) NOT NULL,
  `logtime` bigint(20) unsigned NOT NULL,
  `info` blob NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;