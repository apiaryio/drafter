Feature: Validate SourceMap output

  Scenario: Parse a blueprint file into ast YAML with sourcemap
    When I run `drafter -s -t ast blueprint.apib`
    Then the output should contain the content of file "ast.sourcemap.yaml"

  Scenario: Parse a blueprint file into refract YAML with sourcemap
    When I run `drafter -s blueprint.apib`
    Then the output should contain the content of file "refract.sourcemap.yaml"

  Scenario: Parse a blueprint file into ast JSON with sourcemap
    When I run `drafter -s -f json -t ast blueprint.apib`
    Then the output should contain the content of file "ast.sourcemap.json"

  Scenario: Parse a blueprint file into refract JSON with sourcemap
    When I run `drafter -s -f json blueprint.apib`
    Then the output should contain the content of file "refract.sourcemap.json"
