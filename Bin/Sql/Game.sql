CREATE TABLE  `player` (
  `id` bigint(20) NOT NULL,
  `account` varchar(64) NOT NULL,
  `name` varbinary(64) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
