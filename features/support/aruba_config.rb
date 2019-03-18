Aruba.configure do |config|
  bin =  File.join(Dir.pwd, 'bin')
  ENV['PATH'] = "#{bin}#{File::PATH_SEPARATOR}#{ENV['PATH']}"
end
