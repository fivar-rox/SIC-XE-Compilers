PROGRAM STATS
VAR
    SUM,SUMSQ,I,VALUE,MEAN,VARIANCE,I  :   INTEGER;
    VARIANCE : REAL

BEGIN
    SUM :=  0+Y;
    SUMSQ   :=  0.5;
    FOR I   :=  1   TO  100.5 DO
        BEGIN
            READ(VALUE);
            SUM :=  SUM +   VALUE;
            SUMSQ   :=  SUMSQ   +   VALUE*VALUE
        END;
    $    
    MEAN    :=  SUM DIV 100;
    VARIANCE    :=  SUMSQ   DIV 100 -   MEAN*MEAN;
    WRITE(MEAN,VARIANCE)
    XXXX
END.