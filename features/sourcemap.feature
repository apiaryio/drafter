Feature: Validate SourceMap output

  Scenario: Parse a blueprint file into YAML with sourcemap

    When I run `drafter -s sourcemap blueprint.apib`
    Then a file named "sourcemap" should exist
    When I run `cat sourcemap`
    Then the output should contain the content of file "sourcemap.yaml"
    When I remove the file "sourcemap"
    Then a file named "sourcemap" should not exist

  Scenario: Parse a blueprint file into JSON with sourcemap

    When I run `drafter -s sourcemap --format json blueprint.apib`
    Then a file named "sourcemap" should exist
    When I run `cat sourcemap`
    Then the output should contain the content of file "sourcemap.json"
    When I remove the file "sourcemap"
    Then a file named "sourcemap" should not exist

