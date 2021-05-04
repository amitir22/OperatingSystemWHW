#! /bin/bash

smash_exe_path='../smash'

for input_f in *.in; do
  output_path="${input_f%%.*}.out"
  expected_path="${input_f%%.*}.exp.processed"

  command="$smash_exe_path < $input_f > $output_path"

  eval $command

  DIFF=$(diff $output_path $expected_path)

  if ["$DIFF" != ""]; then
    echo 'Success'
    rm -f $output_path
  else
    echo 'Failed: ${output_path}'
  fi
done
