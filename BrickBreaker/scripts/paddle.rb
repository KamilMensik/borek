# frozen_string_literal: true

class Paddle < Borek::Scriptable
  include Area

  MOVEMENT_SPEED = 96

  def on_update(delta)
    position.x += Borek::Input.axis.x * delta * MOVEMENT_SPEED
    position.x = [[position.x, 0].max, 192].min
  end
end
