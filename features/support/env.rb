require 'aruba/cucumber'

Before do
  @dirs << "../../features/fixtures"
  
  ENV['PATH'] = "#{ENV['PWD']}/src#{File::PATH_SEPARATOR}#{ENV['PATH']}"  
end
