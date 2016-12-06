Then /^the output should contain the content of file "(.*)"$/ do |filename|
  expected = nil
  in_current_dir do
    expected = File.read(filename)

    if ENV['GENERATE'] == '1'
      File.write(filename, all_output)
    end
  end

  assert_partial_output(expected, all_output)
end
