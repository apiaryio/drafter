require 'aruba/cucumber'

Before do
  copy File.join(aruba.config.fixtures_path_prefix, 'blueprint.apib'), 'blueprint.apib'
  copy File.join(aruba.config.fixtures_path_prefix, 'invalid_blueprint.apib'), 'invalid_blueprint.apib'
end
