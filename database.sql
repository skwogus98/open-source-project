DROP DATABASE IF EXISTS `sensorDB` ;

CREATE DATABASE IF NOT EXISTS `sensorDB` 
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;

USE `sensorDB` 

-- -----------------------------------------------------
-- Table `mydb`.`sensorList`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `sensorList` (
  `id` INT NOT NULL auto_increment,
  `name` VARCHAR(255) NULL,
  `cnt` INT NULL,
  `ave` FLOAT NULL,
  `max` FLOAT NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;

/* 새로운 센서 추가
CREATE TABLE IF NOT EXISTS `sensorName` (
  `idx` INT NOT NULL auto_increment,
  `time` FLOAT NULL,
  `value` FLOAT NULL,
  PRIMARY KEY (`idx`))
ENGINE = InnoDB
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;
*/ 
