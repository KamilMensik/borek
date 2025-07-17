# frozen_string_literal: true

# Player2 paddle
class PaddleR < Borek::Scriptable
  def on_update(delta)
    axis = Borek::Input.axis
    transform.position.y += axis.x * 200 * delta
  end
end
