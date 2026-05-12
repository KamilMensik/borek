# frozen_string_literal: true

class Ball < Borek::Scriptable
  include DynamicBody

  MOVEMENT_SPEED = 96

  def on_create
    @score = 0
    @velocity = Vec2[0.3, 1].normalize * MOVEMENT_SPEED

    if BFile.exists? 'score.yaml'
      file = BFile.open('score.yaml', 'r')
      @high_score = (YAML.parse(file).to_rb)['high_score']
    else
      @high_score = 0
    end

    Borek::Scene.root.find_child('HighScore').text = "High Score: #{@high_score}"
  end

  def on_update(delta)
    prev_velocity = @velocity.dup

    col = move_and_collide(@velocity *= delta)
    @velocity /= delta

    return unless col

    if col.normal.y.positive?
      rnd = MOVEMENT_SPEED * (Random.rand - 0.5) / 16
      @velocity = Vec2[prev_velocity.x - rnd, -MOVEMENT_SPEED].normalize * MOVEMENT_SPEED
    elsif col.normal.y.negative?
      rnd = MOVEMENT_SPEED * (Random.rand - 0.5) / 16
      @velocity = Vec2[prev_velocity.x - rnd, MOVEMENT_SPEED].normalize * MOVEMENT_SPEED
    elsif col.normal.x.positive?
      rnd = MOVEMENT_SPEED * (Random.rand - 0.5) / 16
      @velocity = Vec2[-MOVEMENT_SPEED, prev_velocity.y - rnd].normalize * MOVEMENT_SPEED
    elsif col.normal.x.negative?
      rnd = MOVEMENT_SPEED * (Random.rand - 0.5) / 16
      @velocity = Vec2[MOVEMENT_SPEED, prev_velocity.y - rnd].normalize * MOVEMENT_SPEED
    end

    obj = col.obj
    if obj.tags.include? :bricks
      tile = col.data
      puts tile
      obj[tile.x, tile.y] = nil
      @score += 1

      sp = SoundPlayer.new
      sp.asset = asset('sounds/brick_hit.snd')
      sp.on_finished.connect self, :destroy_sound
      sp.play

      Borek::Scene.root.find_child('Score').text = "Score: #{@score}"
    else
      sp = SoundPlayer.new
      sp.asset = asset('sounds/paddle_hit.snd')
      sp.on_finished.connect self, :destroy_sound
      sp.play
    end
  end

  def on_area_entered(args)
    return unless args[:other].name == 'GameLoseArea'

    BFile.open('score.yaml', 'w').write({ 'high_score' => @score }.to_yaml).close if @score > @high_score

    Borek::Scene.restart
  end

  def destroy_sound(args)
    args[:sender].destroy
  end
end
