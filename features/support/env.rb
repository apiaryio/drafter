require 'aruba/cucumber'

Before do
  @dirs << "../../features/fixtures"

  ENV['PATH'] = "#{ENV['DRAFTER_BINARY_DIR']}#{File::PATH_SEPARATOR}#{ENV['PATH']}"  
end
