require 'formula'

class Drafter < Formula
  homepage 'http://apiblueprint.org'
  head 'https://github.com/apiaryio/drafter.git', :tag => 'v2.2.0'

  def install
    system "./configure"
    system "make", "drafter"
    bin.install Dir["bin/drafter"]
  end
end
