perl -ne'$b=unpack("b*",$_);@l=$b=~/.{0,56}/g;
print(join("\n",@l))' <<RM
42
RM

