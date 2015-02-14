Feature: Parse a blueprint

  Scenario: Parse a blueprint file into YAML

    When I run `drafter blueprint.apib`
    Then the output should contain the content of file "ast.yaml"

  Scenario: Parse a blueprint input into YAML

    When I run `drafter` interactively
    When I pipe in the file "blueprint.apib"
    Then the output should contain the content of file "ast.yaml"

  Scenario: Parse a blueprint file into JSON

    When I run `drafter --format=json blueprint.apib`
    Then the output should contain the content of file "ast.json"

  Scenario: Parse a blueprint input into JSON

    When I run `drafter --format=json` interactively
    When I pipe in the file "blueprint.apib"
    Then the output should contain the content of file "ast.json"
