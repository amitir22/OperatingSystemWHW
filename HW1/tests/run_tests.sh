#! /bin/bash

smash_exe_path='../smash'

for input_f in *.in; do
  output_path="${input_f%%.*}.out"
  expected_path="${input_f%%.*}.exp.processed"

  #echo "$input_f"
  #echo $output_path
  #echo $expected_path

  command="$smash_exe_path < $input_f > $output_path"

  eval $command

  DIFF=$(diff $output_path $expected_path)

  if [$DIFF -ne 0]; then
    echo 'Success'
  else
    echo 'Failed'
  fi
done
