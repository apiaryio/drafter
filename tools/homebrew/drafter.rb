require 'formula'

class Drafter < Formula
  homepage 'http://apiblueprint.org'
  head 'https://github.com/apiaryio/drafter.git', :tag => 'v3.2.3'

  def install
    system "./configure", "--shared"
    system "make", "drafter"
    bin.install Dir["bin/drafter"]
    (include + "drafter").install Dir["src/drafter.h"]
    lib.install Dir["build/out/Release/libdrafter.dylib"]
  end

  test do
    system bin/"drafter", "--help"
  end
end
