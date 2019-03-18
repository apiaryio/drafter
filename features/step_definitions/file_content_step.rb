Then /^the output should contain the content of file "(.*)"$/ do |filename|
  expected = nil

  path = File.join(aruba.config.fixtures_directories[0], filename)
  expected = File.read(path)

  if ENV['GENERATE'] == '1'
    File.write(path, all_output)
  end

  assert_partial_output(expected, all_output)
end
