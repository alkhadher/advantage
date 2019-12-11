// *****************************************************
// Copyright (c) 2008-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 1988-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************
IF EXISTS( SELECT * FROM SYSTEM.TABLES WHERE "name" = 'test') THEN
  DROP TABLE "test";
END IF;

IF EXISTS( SELECT * FROM SYSTEM.TABLES WHERE "name" = 'types') THEN
  DROP TABLE "types";
END IF;

CREATE TABLE "test" (
  "id" AUTOINC CONSTRAINT NOT NULL,
  PRIMARY KEY("id")
);

CREATE TABLE "types" (
  "id" INTEGER PRIMARY KEY,
  "_blob_" blob,
  "_numeric_" NUMERIC(5,2),
  "_char_" CHAR(255),
  "_memo_" MEMO,
  "_int_" INTEGER,
  "_short_" SHORT,
  "_logical_" LOGICAL,
  "_date_" DATE,
  "_timestamp_" TIMESTAMP,
  "_double_" DOUBLE,
  "_money_" MONEY
);

INSERT INTO types VALUES 
( 0,
  CHAR2HEX ( '78' ),
  1.1,
  'Bounded String Test',
  'Unbounded String Test',
  1,
  1,
  TRUE,
  '1999-01-02',
  '1999-01-02 21:20:53',
  3.402823e+38,
  1
);

INSERT INTO types VALUES 
( 1,
  CHAR2HEX ( 'FF'), 
  -1.1,
  '',
  '',
  0,
  0,
  FALSE,
  NULL,
  NULL,
  -3.402823e+38,
  -7.77 
);


