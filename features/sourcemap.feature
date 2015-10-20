Feature: Validate SourceMap output

  Scenario: Parse a blueprint file into ast YAML with sourcemap
    When I run `drafter -s sourcemap -t ast blueprint.apib`
    Then a file named "sourcemap" should exist
    And the file "sourcemap" should be equal to file "sourcemap.yaml"
    When I remove the file "sourcemap"
    Then a file named "sourcemap" should not exist

  Scenario: Parse a blueprint file into ast YAML with refract sourcemap
    When I run `drafter -r -t ast blueprint.apib`
    Then a file named "sourcemap" should not exist

  Scenario: Parse a blueprint file into Refract YAML with sourcemap
    When I run `drafter -s sourcemap blueprint.apib`
    Then a file named "sourcemap" should not exist
    And the output should contain the content of file "refract.sm.yaml"

  Scenario: Parse a blueprint file into refract YAML with refract sourcemap
    When I run `drafter -r blueprint.apib`
    Then a file named "sourcemap" should not exist
    And the output should contain the content of file "refract.sm.yaml"


  Scenario: Parse a blueprint file into JSON with sourcemap
    When I run `drafter -s sourcemap --format json -t ast blueprint.apib`
    Then a file named "sourcemap" should exist
    And the file "sourcemap" should be equal to file "sourcemap.json"
    When I remove the file "sourcemap"
    Then a file named "sourcemap" should not exist

  Scenario: Parse a blueprint file into Refract JSON with sourcemap
    When I run `drafter -s sourcemap -f json -t refract blueprint.apib`
    Then a file named "sourcemap" should not exist

  Scenario: Parse a blueprint file into Refract JSON with sourcemap
    When I run `drafter -s sourcemap -f json blueprint.apib`
    Then a file named "sourcemap" should not exist
    And the output should contain the content of file "refract.sm.json"

  Scenario: Parse a blueprint file into refract JSON with refract sourcemap
    When I run `drafter -r -f json blueprint.apib`
    Then a file named "sourcemap" should not exist
    And the output should contain the content of file "refract.sm.json"
