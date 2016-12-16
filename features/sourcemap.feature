Feature: Validate SourceMap output

  Scenario: Parse a blueprint file into refract YAML with sourcemap
    When I run `drafter -s blueprint.apib`
    Then the output should contain the content of file "refract.sourcemap.yaml"

  Scenario: Parse a blueprint file into refract JSON with sourcemap
    When I run `drafter -s -f json blueprint.apib`
    Then the output should contain the content of file "refract.sourcemap.json"
