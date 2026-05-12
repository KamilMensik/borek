# frozen_string_literal: true

# Test
class Test < Borek::Scriptable
  include DynamicBody

  MAX_SPEED = 75
  MAX_RUN_SPEED = 135
  MAX_SPRINT_SPEED = 160

  ACCEL = 337.5

  STOP_DECEL = 275
  WALK_DECEL = 462.5
  RUN_DECEL = 885.0

  P_METER_STARTING_SPEED = 131.25
  P_METER_MAX = 1.867

  GRAVITY = 1350.0
  GRAVITY_JUMP_HELD = 675
  JUMP_POWER = 300
  JUMP_POWER_INCR = 9.375

  def on_create
    @velocity = Vec2[0, 0]
    @p_meter = 0
    @coins = 0
    @state = :alive
    @jump_buffer = 0
    @coyote_time = 0

    puts find_child('BGM').play
  end

  def on_update(delta)
    parent.find_child('Camera').find_child('Text').text = "Coins: #{@coins}"

    case @state
    when :alive
      state_alive(delta)
    when :dead
      state_dead(delta)
    when :winning
      state_winning(delta)
    end
  end

  def handle_anim(sprite)
    sprite.anim = if on_floor?
                    @velocity.x.abs.positive? ? :run : :idle
                  else
                    :jump
                  end
  end

  def state_alive(delta)
    input = Borek::Input.axis
    sprite = find_child('Sprite')

    @jump_buffer -= delta
    @coyote_time -= delta
    @jump_buffer = 0.05 if Borek::Input.key_just_pressed?(:w)
    @coyote_time = 0.1 if on_floor?

    handle_walking(delta)

    if @coyote_time.positive? && @jump_buffer.positive?
      @coyote_time = 0
      @velocity.y = jump_speed
      find_child('JumpSound').play
      particles = find_child('SprintParticles')
      particles.emmits_per_second = 400
      particles.enabled = true
    end

    @velocity.y -= (input.y.positive? ? GRAVITY_JUMP_HELD : GRAVITY) * delta

    sprite.flip_x = input.x.negative? unless input.x.zero?

    col = move_and_collide(@velocity *= delta)
    if col&.obj&.tags&.include?(:map) && col.normal.y.positive?
      data = col.data
      tile = col.obj[data.x, data.y]

      if tile == 60
        col.obj[data.x, data.y] = 58
        sp = SoundPlayer.new
        sp.asset = asset('sounds/Mario Coin Sound - Sound Effect HD.snd')
        sp.on_finished.connect self, :on_sound_finished
        sp.add_tag :ccs
        sp.play
        @coins += 1
      end
    end

    @velocity /= delta

    handle_anim sprite

    parent.find_child('Camera').position.x = position.x

    die if position.y < -120
  end

  def state_dead(delta)
    @timer ||= 3.0

    find_child('Sprite').anim = :dead

    if @timer > 1
      position.y += 300 * delta * (@timer - 2.25)
    else
      position.y -= 300 * delta * (2.25 - @timer) / 2
    end

    scl = 1 + (3 - @timer) / 4
    scale.x = scl
    scale.y = scl

    @timer -= delta

    Borek::Scene.restart if @timer.negative?
  end

  def state_winning(delta)
    sprite = find_child('Sprite')
    sprite.flip_x = false

    if position.y > -64
      position.y -= 300 * delta
      return
    else
      position.y = -64
    end

    flagtop = parent.find_child('FlagTop')

    if flagtop.position.y > -64
      flagtop.position.y -= 64 * delta
      sprite.anim = :idle
    elsif (flagtop.position.x - position.x).abs > 128
      Borek::Scene.change(flagtop.destination)
    else
      flagtop.position.y = -64
      sprite.anim = :run
      position.x += 30 * delta
    end
  end

  def die
    @state = :dead
    bgm = find_child('BGM')
    bgm.asset = asset('sounds/mario_death_sound.snd')
    bgm.play
  end

  def handle_walking(delta)
    input = Borek::Input.axis
    handle_sprint_particles

    return if !on_floor? && input.x.zero?

    if @velocity.x.abs < max_speed && holding_forward_direction?
      accelerate(delta)
    elsif holding_backward_direction?
      decelerate(delta, running? ? RUN_DECEL : WALK_DECEL)
    else
      decelerate(delta, STOP_DECEL)
    end

    return @p_meter = [@p_meter + delta, P_METER_MAX].min if @velocity.x.abs > P_METER_STARTING_SPEED

    @p_meter = [@p_meter - delta, 0].max
  end

  def handle_sprint_particles
    particle_emmiter = find_child('SprintParticles')

    if !on_floor?
      particle_emmiter.enabled = false
    elsif holding_backward_direction?
      particle_emmiter.enabled = true
      particle_emmiter.emmits_per_second = 40
    elsif holding_forward_direction? && p_meter_full?
      particle_emmiter.enabled = true
      particle_emmiter.emmits_per_second = 8 + @velocity.x.abs / 80
    else
      particle_emmiter.enabled = false
    end
  end

  def holding_forward_direction?
    input = Borek::Input.axis

    return false if input.x.zero?

    input.x.positive? ? !@velocity.x.negative? : !@velocity.x.positive?
  end

  def holding_backward_direction?
    input = Borek::Input.axis

    return false if input.x.zero?

    input.x.positive? ? @velocity.x.negative? : @velocity.x.positive?
  end

  def accelerate(delta)
    input = Borek::Input.axis

    @velocity.x += input.x * delta * ACCEL
  end

  def decelerate(delta, amount)
    return if @velocity.x.zero?
    return @velocity.x = [@velocity.x + amount * -delta, 0].max if @velocity.x.positive?

    @velocity.x = [@velocity.x + amount * delta, 0].min
  end

  def max_speed
    return MAX_SPRINT_SPEED if p_meter_full?
    return MAX_RUN_SPEED if running?

    MAX_SPEED
  end

  def jump_speed
    JUMP_POWER + JUMP_POWER_INCR * @velocity.x.abs / 150
  end

  def p_meter_full?
    @p_meter >= 1.5
  end

  def running?
    Borek::Input.key_pressed? :left_shift
  end

  def on_sound_finished(args)
    sender = args[:sender]
    sender.destroy if sender.tags.include? :ccs
  end

  def on_body_entered(args)
    return unless @state == :alive
    return unless args[:other].tags.include?(:enemy)

    return unless args[:other].alive

    if !@velocity.y.negative?
      die
    else
      @velocity.y = JUMP_POWER / 2
      args[:other].alive = false
      @coins += 1
    end
  end

  def on_body_exited(args); end

  def on_area_entered(args)
    if args[:other].tags.include?(:coin)
      args[:other].destroy

      @coins += 1

      sp = SoundPlayer.new
      sp.asset = asset('sounds/Mario Coin Sound - Sound Effect HD.snd')
      sp.on_finished.connect self, :on_sound_finished
      sp.add_tag :ccs
      sp.play
    elsif args[:other].tags.include?(:end)
      find_child('BGM').asset = asset('sounds/win_sound.snd')
      find_child('BGM').play
      
      @velocity.x = 0
      @state = :winning
    end
  end

  def on_area_exited(args); end
end
