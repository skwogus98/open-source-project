DROP DATABASE IF EXISTS `musicDB` ;

CREATE DATABASE IF NOT EXISTS `sensorDB` 
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;

USE `sensorDB` 

-- -----------------------------------------------------
-- Table `mydb`.`artistDB`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `artistDB` (
  `id` INT NOT NULL auto_increment,
  `name` VARCHAR(255) NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;

-- -----------------------------------------------------
-- Table `mydb`.`albumDB`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `albumDB` (
  `id` INT NOT NULL auto_increment,
  `title` VARCHAR(255) NULL,
  `artistDB_id` INT NOT NULL,
  PRIMARY KEY (`id`),
  FOREIGN KEY (`artistDB_id`) REFERENCES `artistDB` (`id`)
    ON DELETE CASCADE ON UPDATE CASCADE
	)
ENGINE = InnoDB
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;


-- -----------------------------------------------------
-- Table `mydb`.`userDB`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `userDB` (
  `id` INT NOT NULL auto_increment,
  `userID` VARCHAR(255) NOT NULL,
  `password` VARCHAR(255) NOT NULL,
  `nickname` VARCHAR(255),
  PRIMARY KEY (`id`))
ENGINE = InnoDB
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;
  
 
-- -----------------------------------------------------
-- Table `mydb`.`list`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `rateListDB` (
  `id` INT NOT NULL auto_increment,
  `rank` INT NULL,
  `albumDB_id` INT NOT NULL,
  `userDB_id` INT NOT NULL,
  PRIMARY KEY (`id`),
  FOREIGN KEY (`albumDB_id`) REFERENCES `albumDB` (`id`)
    ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (`userDB_id`) REFERENCES `userDB` (`id`)
    ON DELETE CASCADE ON UPDATE CASCADE)
ENGINE = InnoDB
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;


-- -----------------------------------------------------
-- Table `mydb`.`songDB`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `songDB` (
  `id` INT NOT NULL auto_increment,
  `title` VARCHAR(255),
  `trackNo` INT,
  `albumDB_id` INT,
  PRIMARY KEY (`id`),
  FOREIGN KEY (`albumDB_id`) REFERENCES `albumDB` (`id`)
    ON DELETE CASCADE ON UPDATE CASCADE)
ENGINE = InnoDB
  DEFAULT CHARACTER SET utf8 
  DEFAULT COLLATE utf8_general_ci;