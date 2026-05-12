# frozen_string_literal: true

# Menim tohle
class Enemy < Borek::Scriptable
  include DynamicBody

  SPEED = 50
  GRAVITY = 1300

  def initialize
    @velocity = Vec2[0, 0]
    @coins = 0
    @dir = -1
    super
  end

  def on_update(delta)
    sprite = find_child('Sprite')
    death_sound = find_child('DeathSound')

    if alive?
      sprite.anim = :moving

      @velocity.x = @dir * SPEED
      @velocity.y -= GRAVITY * delta

      move_and_collide(@velocity *= delta)

      @dir *= -1 if @velocity.x.zero?

      @velocity /= delta

      destroy if position.y < -500
    else
      sprite.anim = :dead
      death_sound.play unless death_sound.playing?
    end
  end

  def alive?
    alive
  end

  def on_death_sound_end(_args)
    destroy
  end
end
