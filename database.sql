DROP DATABASE IF EXISTS `musicDB` ;

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