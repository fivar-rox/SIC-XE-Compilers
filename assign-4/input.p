program stats
var
    sum,sumsq,i,value,mean,variance  :   integer

begin
    sum :=  0;
    sumsq   :=  1;
    for i   :=  1   to  100 do
        begin
            read(value);
            sum :=  sum +   value;
            sumsq   :=  sumsq   +   value*value
        end;
        
    mean    :=  sum div 100;
    variance    :=  sumsq   div 100 -   mean*mean;
    write(mean,variance)
end.