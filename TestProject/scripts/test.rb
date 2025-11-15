# frozen_string_literal: true

# Test
class Test < Borek::Scriptable
  SPEED = 400.0
  JUMP_FORCE = 750.0
  GRAVITY = 1050.0
  GRAVITY2 = 1550.0

  attr_accessor :velocity

  def initialize
    @velocity = Borek::Vec2.new(0, 0)
    super
  end

  def on_update(delta)
    axis = Borek::Input.axis
    velocity.x = delta * SPEED * axis.x

    if axis.y.positive? && on_floor?
      velocity.y = JUMP_FORCE
      find_child('JumpSound').get_component(Borek::SoundPlayerComponent).play
    end

    velocity.y -= (axis.y.positive? ? GRAVITY : GRAVITY2) * delta

    find_child('Sprite').get_component(Borek::SpriteComponent).flip_x = axis.x.negative? unless axis.x.zero?

    # Hack to keep the jump height consistent
    velocity.y *= delta
    move_and_collide(velocity)
    velocity.y /= delta

    Borek::Scene.find_child('Camera').transform.position.x = transform.position.x
  end
end
