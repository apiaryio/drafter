Feature: Parse a blueprint

  Scenario: Parse a blueprint file into Refract YAML

    When I run `drafter blueprint.apib`
    Then the output should contain the content of file "refract.yaml"

  Scenario: Parse a blueprint input into Refract YAML

    When I run `drafter` interactively
    When I pipe in the file "blueprint.apib"
    Then the output should contain the content of file "refract.yaml"

  Scenario: Parse a blueprint file into Refract JSON

    When I run `drafter -f json blueprint.apib`
    Then the output should contain the content of file "refract.json"

  Scenario: Parse a blueprint input into Refract JSON

    When I run `drafter -f json` interactively
    When I pipe in the file "blueprint.apib"
    Then the output should contain the content of file "refract.json"
