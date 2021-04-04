-- Structure for table custom_dwrath_character_stats
CREATE TABLE IF NOT EXISTS `custom_dwrath_character_stats` (
	`GUID` TINYINT(3) UNSIGNED NOT NULL,
	`Difficulty` FLOAT NOT NULL DEFAULT '0',
	`GroupSize` INT(11) NOT NULL DEFAULT '1',
	`SpellPower` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`Stats` FLOAT NOT NULL DEFAULT '100',
	`GroupLevelTog` TINYINT(3) NULL DEFAULT '0',
	`BoostedXP` INT(11) UNSIGNED NOT NULL DEFAULT '1',
	`d_map` SMALLINT(6) NOT NULL DEFAULT '-1',
	`d_x` FLOAT NOT NULL DEFAULT '0',
	`d_y` FLOAT NOT NULL DEFAULT '0',
	`d_z` FLOAT NOT NULL DEFAULT '0',
	`typed` VARCHAR(30) NULL DEFAULT '0' COLLATE 'utf8_general_ci',
	PRIMARY KEY (`GUID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
