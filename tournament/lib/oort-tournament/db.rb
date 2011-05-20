require 'yaml'

module OortTournament

class DB
  def initialize filename="tournament/oort-tournament-db.yaml"
    @filename = filename
    if File.exists? @filename
      @db = File.open(@filename) { |io| YAML.load io }
    else
      @db = {
        'seen_msgids' => [],
        'ais' => {},
      }
    end
  end
    
  def save
    tmp_filename = File.join(File.dirname(@filename), ".#{File.basename(@filename)}.tmp")
    File.open(tmp_filename, 'w') { |io| YAML.dump @db, io }
    File.rename tmp_filename, @filename
  end

  def method_missing sym, *args
    if @db.member? sym.to_s and args.length == 0
      @db[sym.to_s]
    else
      super
    end
  end

  def create_ai name, user, gist
    fail "already exists" if ais.member? name
    ais[name] = {
      'user' => user,
      'gist' => gist,
      'mean' => 25,
      'deviation' => 25 / 3.0,
      'activity' => 1,
    }
  end
end

end
